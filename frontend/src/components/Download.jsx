import { motion } from 'framer-motion';
import { useInView } from 'react-intersection-observer';
import { FaWindows, FaGithub, FaDownload } from 'react-icons/fa';

const downloads = [
  {
    title: 'Windows Installer',
    description: 'Latest stable release for Windows',
    icon: <FaWindows className="w-8 h-8" />,
    version: 'v1.0',
    size: '2.72 MB',
    url: 'https://github.com/Nischaya008/RayTracerNG/releases/download/RayTracerNG/RayTracerNG_Setup.exe',
  },
  {
    title: 'Source Code',
    description: 'Build from source with CMake',
    icon: <FaGithub className="w-8 h-8" />,
    version: 'v1.0',
    size: '1.67 MB',
    url: 'https://github.com/Nischaya008/RayTracerNG/archive/refs/tags/RayTracerNG.zip',
  },
];

const DownloadCard = ({ title, description, icon, version, size, url, index }) => {
  const [ref, inView] = useInView({
    triggerOnce: true,
    threshold: 0.1,
  });

  return (
    <motion.div
      ref={ref}
      initial={{ opacity: 0, y: 50 }}
      animate={inView ? { opacity: 1, y: 0 } : { opacity: 0, y: 50 }}
      transition={{ duration: 0.5, delay: index * 0.1 }}
      className="glass p-6 rounded-xl hover:scale-105 transition-transform duration-300"
    >
      <div className="text-accent mb-4">{icon}</div>
      <h3 className="text-xl font-display font-bold mb-2">{title}</h3>
      <p className="text-gray-300 text-sm mb-4">{description}</p>
      <div className="flex items-center justify-between text-sm text-gray-400 mb-4">
        <span>Version: {version}</span>
        <span>Size: {size}</span>
      </div>
      <a
        href={url}
        className="btn btn-primary w-full flex items-center justify-center gap-2"
      >
        <FaDownload />
        Download
      </a>
    </motion.div>
  );
};

const Download = () => {
  return (
    <section id="download" className="section">
      <div className="container">
        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5 }}
          viewport={{ once: true }}
          className="text-center mb-16"
        >
          <h2 className="text-4xl md:text-5xl font-display font-bold mb-4">
            Download
          </h2>
          <p className="text-gray-300 text-lg">
            Get started with RayTracerNG today
          </p>
        </motion.div>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-8 max-w-4xl mx-auto">
          {downloads.map((download, index) => (
            <DownloadCard key={index} {...download} index={index} />
          ))}
        </div>

        <motion.div
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.5, delay: 0.2 }}
          viewport={{ once: true }}
          className="text-center mt-12"
        >
          <p className="text-gray-400 text-sm">
            System Requirements: Windows 10/11, OpenGL 4.5+ compatible graphics card
          </p>
        </motion.div>
      </div>
    </section>
  );
};

export default Download; 